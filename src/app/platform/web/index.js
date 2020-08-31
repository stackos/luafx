let gl = null;
const Engine = {
    Init: null,
    Done: null,
    Update: null,
};

const Platform = {
    Android: 0,
    iOS: 1,
    Mac: 2,
    Windows: 3,
    Other: 4,
};

function GetPlatform() {
    const agent = navigator.userAgent;
    if (agent.indexOf("Android") >= 0) {
        return Platform.Android;
    } else if (agent.indexOf("iPhone") >= 0 || agent.indexOf("iPad") >= 0 || agent.indexOf("iPod") >= 0) {
        return Platform.iOS;
    } else if (agent.indexOf("Mac") >= 0) {
        return Platform.Mac;
    } else if (agent.indexOf("Windows") >= 0) {
        return Platform.Windows;
    } else {
        return Platform.Other;
    }
}

function Render() {
    const msg = {
        time: new Date().getTime(),
    };

    Engine.Update(JSON.stringify(msg));

    window.requestAnimationFrame(Render);
}

// functions for C call
function LoadFileAsync(user_data, url) {
    console.log("LoadFileAsync user_data:" + user_data + " url:" + url);

	const req = new XMLHttpRequest();
	req.open("GET", url, true);
	req.responseType = "arraybuffer";

	req.onload = function (event) {
        if (req.readyState === 4 && req.status === 200) {
            const buffer = req.response;
            if (buffer) {
			    const bytes = new Uint8Array(buffer);
			    const p = Engine.MemoryAlloc(bytes.length);
			    Module.HEAP8.set(bytes, p);
			    Engine.OnLoadFileAsyncComplete(user_data, url, p, bytes.length);
			    Engine.MemoryFree(p);
		    } else {
                Engine.OnLoadFileAsyncComplete(user_data, url, null, 0);
            }
        } else {
            console.log("LoadFileAsync failed in readyState:" + req.readyState, "status:" + req.status);

            Engine.OnLoadFileAsyncComplete(user_data, url, null, 0);
        }
	};

	req.send(null);
}
//

function PaintToolDraw(canvas, c) {
    const draw_to_save = false;

    if (draw_to_save) {
        c.fillStyle="#ffffff00";
    } else {
        c.fillStyle="#000000";
    }
    c.fillRect(0, 0, canvas.width, canvas.height);

    c.fillStyle="#ffffff";

    c.beginPath();
    c.arc(100, 100, 64, 0, Math.PI * 2, true);
    c.fill();

    const DrawPoly = function(x, y, r, n) {
        c.beginPath();
        c.moveTo(x, y - r);
        for (var i = 1; i < n; ++i) {
            const t = Math.PI * 2 / n * i;
            const rx = Math.cos(t) * r;
            const ry = Math.sin(t) * r;
            c.lineTo(x - ry, y - rx);
        }
        c.fill();
    };

    DrawPoly(300, 100, 64, 3);
    DrawPoly(500, 100, 64, 4);
    DrawPoly(700, 100, 64, 5);
    DrawPoly(900, 100, 64, 6);
    DrawPoly(1100, 100, 64, 7);

    c.lineCap = "round";
    c.lineJoin = "round";
    c.strokeStyle="#ffffff";

    c.lineWidth = 32;
    c.beginPath();
    c.moveTo(100 - 1, 300);
    c.lineTo(100 + 1, 300);
    c.stroke();

    c.lineWidth = 8;
    c.beginPath();
    c.moveTo(200, 400);
    c.arcTo(200, 200, 600, 392, 8);
    c.arcTo(600, 392, 600, 400, 8);
    c.lineTo(600, 400);
    c.stroke();

    c.lineWidth = 4;
    c.beginPath();
    for (var i = 0; i < 17; ++i)
    {
        const p = 0.6;
        const len = (600 - 200) / (16 + p);
        c.moveTo(600 - len * i, 402);
        c.lineTo(600 - len * i - len * p, 402);
    }
    c.stroke();

    c.fillStyle="#272727";
    c.lineWidth = 4;
    c.beginPath();
    c.arc(900, -750, 1000, (90 - 15) * Math.PI / 180, (90 + 15) * Math.PI / 180);
    c.lineTo(900 - 260, 400);
    c.lineTo(900 + 260, 400);
    c.fill();
    c.fillStyle="#ffffff";

    c.strokeStyle="#565656";
    c.lineWidth = 4;
    c.beginPath();
    c.arc(900, -750, 1000, (90 - 15) * Math.PI / 180, (90 + 15) * Math.PI / 180);
    c.stroke();
    c.strokeStyle="#ffffff";

    for (var i = 0; i < 8; ++i)
    {
        const p = 0.6;
        const len = 360 / 8;
        c.beginPath();
        c.arc(100, 500, 64, (i * len) * Math.PI / 180, ((i + p) * len) * Math.PI / 180);
        c.stroke();
    }

    c.lineWidth = 12;
    c.beginPath();
    c.arc(300, 500, 8, 0, Math.PI * 2, true);
    c.fill();
    c.beginPath();
    for (var i = 0; i < 3; ++i)
    {
        const t = Math.PI * 2 / 3 * i;
        const rx0 = Math.cos(t) * 24;
        const ry0 = Math.sin(t) * 24;
        const rx1 = Math.cos(t) * 64;
        const ry1 = Math.sin(t) * 64;
        c.moveTo(300 + rx0, 500 + ry0);
        c.lineTo(300 + rx1, 500 + ry1);
    }
    c.stroke();

    c.beginPath();
    c.arc(500, 500, 64, 0, Math.PI * 2, true);
    c.fill();
    c.strokeStyle="#000000";
    c.lineWidth = 16;
    c.beginPath();
    c.moveTo(500 - 20, 500 - 30);
    c.lineTo(500 - 20, 500 + 30);
    c.moveTo(500 + 20, 500 - 30);
    c.lineTo(500 + 20, 500 + 30);
    c.stroke();
    c.strokeStyle="#ffffff";
}

function InitLoading() {
    const canvas = document.getElementById("canvas2d");
    const c = canvas.getContext("2d");

    const Draw = function () {
        PaintToolDraw(canvas, c);

        c.font = "40px Arial";
        c.fillStyle = "#ffffff";
        c.textAlign = "center";
        //c.fillText("Loading...", canvas.width / 2, canvas.height - 60);
    };

    SetCanvasSize(canvas, GetPlatform());

    Draw();
}
InitLoading();

function RemoveLoading() {
    const canvas = document.getElementById("canvas2d");
    canvas.remove();
}

function SetCanvasSize(canvas, platform) {
    if (platform === Platform.Android || platform === Platform.iOS || document.body.clientWidth < 1280) {
        canvas.width = document.body.clientWidth;
        canvas.height = canvas.width * 720 / 1280;
    } else {
        canvas.width = 1280;
        canvas.height = 720;
    }
}

function Main() {
    Engine.Init = Module.cwrap("InitEngine", null, ["string"]);
    Engine.Done = Module.cwrap("DoneEngine", null, ["string"]);
    Engine.Update = Module.cwrap("UpdateEngine", null, ["string"]);
    Engine.MemoryAlloc = Module.cwrap("MemoryAlloc", "number", ["number"]);
	Engine.MemoryFree = Module.cwrap("MemoryFree", null, ["number"]);
    Engine.OnLoadFileAsyncComplete = Module.cwrap("OnLoadFileAsyncComplete", null, ["number", "string", "number", "number"]);

	let force_gles2 = true;
    let glesv3 = false;
    const canvas = Module.canvas;
    let context = 0;
	if (!force_gles2) {
		context = GL.createContext(canvas, {
			majorVersion: 2,
			minorVersion: 0,
			antialias: false,
			alpha: false
		});
	}
    if (context === 0) {
        context = GL.createContext(canvas, {
            majorVersion: 1,
            minorVersion: 0,
            antialias: false,
            alpha: false
        });
    } else {
        glesv3 = true;
    }
    GL.makeContextCurrent(context);
    gl = Module.ctx;

    const platform = GetPlatform();
    const has_touch = !!(('ontouchstart' in window) || (window.DocumentTouch && document instanceof DocumentTouch));

    console.log(navigator.userAgent);
    
    SetCanvasSize(canvas, platform);

    const canvas_width = canvas.width;
    const canvas_height = canvas.height;

    const msg = {
        name: "luafx",
        width: canvas_width,
        height: canvas_height,
        glesv3: glesv3,
        platform: platform,
        time: new Date().getTime(),
    };

    Engine.Init(JSON.stringify(msg));

    Render();

    //RemoveLoading();
    //canvas.style = "display: initial;";
}

var Module = {
    preRun: [],
    postRun: function () {
        Main();
    },
    print: (function () {
        return function (text) {
            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
            console.log(text);
        };
    })(),
    printErr: function (text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
        if (0) { // XXX disabled for safety typeof dump == 'function') {
            dump(text + '\n'); // fast, straight to the real console
        } else {
            console.error(text);
        }
    },
    canvas: (function () {
        var canvas = document.getElementById("canvas");

        // As a default initial behavior, pop up an alert when webgl context is lost. To make your
        // application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas.addEventListener("webglcontextlost", function (e) {
            alert("WebGL context lost. You will need to reload the page.");
            e.preventDefault();
        }, false);

        return canvas;
    })()
};

window.onerror = function (event) {
    Module.setStatus = function (text) {
        if (text) Module.printErr("[post-exception status] " + text);
    };
};
