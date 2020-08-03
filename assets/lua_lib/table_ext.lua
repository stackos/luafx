local function value_to_string(v, depth)
    local type_v = type(v)
    if type_v == "number" then
        return tostring(v)
    elseif type_v == "string" then
        return v
    elseif type_v == "boolean" then
        return tostring(v)
    elseif type_v == "table" then
        local str = "{\n"

        for k,tv in pairs(v) do
            for i = 1, depth + 1 do
                str = str .. "\t"
            end

            local key_type = type(k)
            if key_type == "number" then
                str = str .. string.format("[%d]: ", k)
            elseif key_type == "string" then
                str = str .. string.format("\"%s\": ", k)
            end

            if type(tv) == "string" then
                str = str .. string.format("\"%s\",\n", value_to_string(tv, depth + 1))
            else
                str = str .. string.format("%s,\n", value_to_string(tv, depth + 1))
            end
        end

        for i = 1, depth do
            str = str .. "\t"
        end
        str = str .. "}"
        return str
    else
        return type_v
    end
end

table.dump = function(t, depth)
    if type(t) ~= "table" then
        return ""
    end

    if depth == nil then
        depth = 0
    end

    return value_to_string(t, depth)
end
