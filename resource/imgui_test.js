/// <reference path=".config/sa.d.ts" />

// 
// ImGuiRedux Example Script
// 

const TOGGLE_KEY = 116 // F5
var gShowWindow = false
var gVersionInfo = false
var gMainMenuBar = false
var gRadioBtn = 1

while (true) 
{
    /*
        This must always be 0 to render ImGui each frame
        If you need to wait for something use timers instead, 
        example below
    */
    wait(0) 


    /*
        All ImGui code must be wrapped inside the BeginFrame() .. EndFrame()
        You need to pass a "UNIQUE ID" for the frame name.
        Use something that's unlikely to be used by any other scripts
    */
    ImGui.BeginFrame("IMGUI_DEMO")
    ImGui.SetCursorVisible(gShowWindow)

    // Crashes atm
    // if (gMainMenuBar)
    // {
    //     if (ImGui.BeginMainMenuBar("MMB"))
    //     {
    //         if (ImGui.BeginMenu("File", true))
    //         {
    //             ImGui.MenuItem("New")
    //             ImGui.MenuItem("Close")
    //             ImGui.EndMenu();
    //         }
    //         if (ImGui.BeginMenu("Edit", true))
    //         {
    //             ImGui.MenuItem("Option 1")
    //             ImGui.MenuItem("Option 2")
    //             ImGui.EndMenu();
    //         }
    //         ImGui.EndMainMenuBar()
    //     }
    // }

    if (gShowWindow)
    {
        ImGui.SetNextWindowSize(350, 600, 2) // 2 = ImGuiCond_Once
        let windowValue = ImGui.Begin("ImGuiRedux Demo Window", gShowWindow, 0)

        let reduxVer = ImGui.GetReduxVersion()
        let imguiVer = ImGui.GetVersion()
        let fps = ImGui.GetFramerate()
        

        // A basic column example
        gVersionInfo = ImGui.Checkbox("Show version info", gVersionInfo)
        gMainMenuBar = ImGui.Checkbox("Show main menubar", gMainMenuBar)

        if (gVersionInfo)
        {   
            ImGui.Spacing()
            ImGui.Columns(2)
            ImGui.Text("ImGui: " + imguiVer)
            ImGui.Text("Framerate: " + fps)
            ImGui.NextColumn()
            ImGui.Text("Redux: " + reduxVer.toFixed(2))
            ImGui.Columns(1)
        }

        ImGui.Dummy(0, 20)

        
        ImGui.BeginChild("WindowChild")
        // Scaling example with buttons
        if (ImGui.CollapsingHeader("Buttons"))
        {
            let size = ImGui.GetScalingSize("S0", 2, false)
            if (ImGui.Button("Normal button 1", size.x, size.y))
            {
                showTextBox("Normal button 1 was pressed")
            }
            ImGui.SameLine()
            if (ImGui.Button("Normal button 2", size.x, size.y))
            {
                showTextBox("Normal button 2 was pressed")
            }

            ImGui.Spacing()
            
            size = ImGui.GetScalingSize("S1", 3, false)
            if (ImGui.ColorButton("CB 1", 1.0, 0.0, 0.0, 1.0, size.x, size.y))
            {
                showTextBox("Color button 1 was pressed")
            }
            ImGui.SameLine()
            if (ImGui.ColorButton("CB 2", 0.0, 1.0, 0.0, 1.0, size.x, size.y))
            {
                showTextBox("Color button 2 was pressed")
            }
            ImGui.SameLine()
            if (ImGui.ColorButton("CB 3", 0.0, 0.0, 1.0, 1.0, size.x, size.y))
            {
                showTextBox("Color button 3 was pressed")
            }
            
            size = ImGui.GetScalingSize("S2", 1, false)
            ImGui.Text("Tip: Click the below area")
            if (ImGui.InvisibleButton("Invis button", size.x, size.y))
            {
                showTextBox("Invisible button was pressed")
            }
            if (ImGui.IsItemHovered("HOver"))
            {
                ImGui.Text("Hovering invisible button")
            }

            
            ImGui.Spacing()

            // Radio button
            gRadioBtn = ImGui.RadioButton("RB1", gRadioBtn, 1)
            ImGui.SameLine()
            gRadioBtn = ImGui.RadioButton("RB2", gRadioBtn, 2)
            ImGui.SameLine()
            gRadioBtn = ImGui.RadioButton("RB3", gRadioBtn, 3)
            ImGui.Spacing()
            let str = "Current Button: " + gRadioBtn
            ImGui.Text(str)

            ImGui.Separator()
            ImGui.Spacing()
        }

        ImGui.Spacing()

        // Input and Slider example
        if (ImGui.CollapsingHeader("Inputs"))
        {
            ImGui.InputInt("Int input", 420, 0, 1000);
            ImGui.InputFloat("Float input", 69.9, 0.0, 100.0)
            ImGui.Spacing()
            ImGui.SliderInt("Int slider", 5, 0, 10)
            ImGui.SliderFloat("Float slider", 5.0, 0.0, 10.0)
            ImGui.Spacing()
            let text = ImGui.InputText("Text input widget")
            ImGui.Spacing()
            var col = ImGui.ColorPicker("Color Picker", 0)
            ImGui.ColorButton("COLORBTN", col.red, col.green, col.blue, col.alpha, 70.0, 30.0)

            ImGui.Dummy(0, 10)
            ImGui.Text("Text input: " + text)
            

            ImGui.Separator()
            ImGui.Spacing()
        }
        
        ImGui.Spacing()

        if (ImGui.CollapsingHeader("Texts"))
        {
            ImGui.Text("A little brown fox jumps over the lazy dog")
            ImGui.TextDisabled("A little brown fox jumps over the lazy dog")
            ImGui.TextColored("A little brown fox jumps over the lazy dog", 1.0, 0.0, 0.0, 1.0)
            ImGui.BulletText("A little brown fox jumps over the lazy dog")
            ImGui.Spacing()
            ImGui.TextWrapped("A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog")
            ImGui.Separator()
            ImGui.Spacing()
        } 
            
        ImGui.EndChild()
    
        ImGui.End()
        gShowWindow = windowValue.isOpen
    }  
    ImGui.EndFrame()  



    if (Pad.IsKeyPressed(TOGGLE_KEY)) 
    {
        gShowWindow = !gShowWindow
        while (Pad.IsKeyPressed(TOGGLE_KEY)) 
        {
            wait(10)
        }
    }
}


