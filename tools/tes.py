import json

lst = {
    "IMGUI_BEGIN_FRAME" : 0x2200,
    "IMGUI_END_FRAME" : 0x2201,
    "IMGUI_BEGIN" : 0x2202,
    "IMGUI_END" : 0x2203,
    "IMGUI_BEGIN_MAINMENUBAR" : 0x2204,
    "IMGUI_END_MAINMENUBAR" : 0x2205,
    "IMGUI_BEGIN_CHILD" : 0x2206,
    "IMGUI_END_CHILD" : 0x2207,
    "IMGUI_TABS" : 0x2208,
    "IMGUI_COLLAPSING_HEADER" : 0x2209,
    "IMGUI_SET_WINDOW_POS" : 0x220A,
    "IMGUI_SET_WINDOW_SIZE" : 0x220B,
    "IMGUI_SET_NEXT_WINDOW_POS" : 0x220C,
    "IMGUI_SET_NEXT_WINDOW_SIZE" : 0x220D,
    "IMGUI_TEXT" : 0x220E,
    "IMGUI_TEXT_CENTERED" : 0x220F,
    "IMGUI_TEXT_DISABLED" : 0x2210,
    "IMGUI_TEXT_WRAPPED" : 0x2211,
    "IMGUI_TEXT_COLORED" : 0x2212,
    "IMGUI_BULLET_TEXT" : 0x2213,
    "IMGUI_BULLET" : 0x2214,
    "IMGUI_CHECKBOX" : 0x2215,
    "IMGUI_COMBO" : 0x2216,
    "IMGUI_SET_TOOLTIP" : 0x2217,
    "IMGUI_BUTTON" : 0x2218,
    "IMGUI_IMAGE_BUTTON" : 0x2219,
    "IMGUI_INVISIBLE_BUTTON" : 0x221A,
    "IMGUI_COLOR_BUTTON" : 0x221B,
    "IMGUI_ARROW_BUTTON" : 0x221C,
    "IMGUI_SLIDER_INT" : 0x221D,
    "IMGUI_SLIDER_FLOAT" : 0x221E,
    "IMGUI_INPUT_INT" : 0x221F,
    "IMGUI_INPUT_FLOAT" : 0x2220,
    "IMGUI_INPUT_TEXT" : 0x2221,
    "IMGUI_RADIO_BUTTON" : 0x2222,
    "IMGUI_COLOR_PICKER" : 0x2223,
    "IMGUI_MENU_ITEM" : 0x2224,
    "IMGUI_SELECTABLE" : 0x2225,
    "IMGUI_DUMMY" : 0x2226,
    "IMGUI_SAMELINE" : 0x2227,
    "IMGUI_NEWLINE" : 0x2228,
    "IMGUI_COLUMNS" : 0x2229,
    "IMGUI_NEXT_COLUMN" : 0x222A,
    "IMGUI_SPACING" : 0x222B,
    "IMGUI_SEPARATOR" : 0x222C,
    "IMGUI_PUSH_ITEM_WIDTH" : 0x222D,
    "IMGUI_POP_ITEM_WIDTH" : 0x222E,
    "IMGUI_IS_ITEM_ACTIVE" : 0x222F,
    "IMGUI_IS_ITEM_CLICKED" : 0x2230,
    "IMGUI_IS_ITEM_FOCUSED" : 0x2231,
    "IMGUI_IS_ITEM_HOVERED" : 0x2232,
    "IMGUI_SET_ITEM_INT" : 0x2233,
    "IMGUI_SET_ITEM_FLOAT" : 0x2234,
    "IMGUI_SET_ITEM_TEXT" : 0x2235,
    "IMGUI_SET_IMAGE_BG_COLOR" : 0x2236,
    "IMGUI_SET_IMAGE_TINT_COLOR" : 0x2237,
    "IMGUI_LOAD_IMAGE" : 0x2238,
    "IMGUI_FREE_IMAGE" : 0x2239,
    "IMGUI_PUSH_STYLE_VAR" : 0x223A,
    "IMGUI_PUSH_STYLE_VAR2" : 0x223B,
    "IMGUI_PUSH_STYLE_COLOR" : 0x223C,
    "IMGUI_POP_STYLE_VAR" : 0x223D,
    "IMGUI_POP_STYLE_COLOR" : 0x223E,
    "IMGUI_GET_FOREGROUND_DRAWLIST" : 0x223F,
    "IMGUI_GET_BACKGROUND_DRAWLIST" : 0x2240,
    "IMGUI_GET_WINDOW_DRAWLIST" : 0x2241,
    "IMGUI_DRAWLIST_ADD_TEXT" : 0x2242,
    "IMGUI_DRAWLIST_ADD_LINE" : 0x2243,
    "GET_FRAMERATE" : 0x2244,
    "IMGUI_GET_VERSION" : 0x2245,
    "IMGUI_GET_PLUGIN_VERSION" : 0x2246,
    "IMGUI_SET_CURSOR_VISIBLE" : 0x2247,
    "IMGUI_GET_FRAME_HEIGHT" : 0x2248,
    "IMGUI_GET_WINDOW_POS" : 0x2249,
    "IMGUI_GET_WINDOW_SIZE" : 0x224A,
    "IMGUI_CALC_TEXT_SIZE" : 0x224B,
    "IMGUI_GET_WINDOW_CONTENT_REGION_WIDTH" : 0x224C,
    "IMGUI_GET_SCALING_SIZE" : 0x224D,
    "IMGUI_GET_DISPLAY_SIZE" : 0x224E,
    "IMGUI_SET_NEXT_WINDOW_TRANSPARENCY" : 0x224F,
    "IMGUI_SET_MESSAGE" : 0x2250
}

# Retrieve JSON data from the file
with open("E:/ImGuiRedux/tools/test.json", "r") as f:
    data = json.load(f)

    outdata = []
    with open("E:/ImGuiRedux/tools/out.json", "w") as w:
        for e in data["commands"]:
            e["id"] = str(hex(lst[e["name"]])).replace("0x", "").upper();
            outdata.insert(-1, e);
        formatted_data = json.dumps(outdata, indent=2)

        w.write(formatted_data)