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

function InitLoading() {
    const canvas = document.getElementById("canvas2d");
    const c = canvas.getContext("2d");

    const draw = function () {
        c.font = "40px Arial";
        c.fillStyle = "#ffffff";
        c.textAlign = "center";
        c.fillText("Loading...", canvas.width / 2, canvas.height - 60);
    };

    SetCanvasSize(canvas, GetPlatform());

    draw();
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

    RemoveLoading();
    canvas.style = "display: initial;";
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
