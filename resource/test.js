/// <reference path=".config/sa.d.ts" />
var VK_F5 = 116
var isOpen = true
var gCheckbox = 0
var radioBtn = 1

while (true) 
{
    wait(0) // always 0
    
    ImGui.BeginFrame("MyWindow")
    ImGui.SetCursorVisible(isOpen)
    if (isOpen)
    {
        var windowValue = ImGui.Begin("MyWindow", isOpen)
        

        // Checkbox
        gCheckbox = ImGui.Checkbox("Show sample text", gCheckbox)
        if (gCheckbox)
        {   
            ImGui.Text("THIS IS A TEST")
        }
        
        // Input and Slider
        var float_ = ImGui.InputFloat("INPUT", 69.9, 0.0, 100.0)
        if(ImGui.IsItemHovered("RandomText"))
        {
            ImGui.Text("Hovered!")
        }
        var slider = ImGui.SliderInt("Slider", 5, 0, 10)
        var str = "Input: " + float_.toFixed(2) + " Slider: " + slider
        ImGui.Text(str) 

        var text = ImGui.InputText("TEST")
        ImGui.Text(text)


        // Radio button
        radioBtn = ImGui.RadioButton("RB1", radioBtn, 1)
        ImGui.SameLine()
        radioBtn = ImGui.RadioButton("RB2", radioBtn, 2)
        ImGui.SameLine()
        radioBtn = ImGui.RadioButton("RB3", radioBtn, 3)
        ImGui.Spacing()
        str = "Current Button: " + radioBtn
        ImGui.Text(str)


        // Color button and picker
        var col = ImGui.ColorPicker("Color Picker", 0)
        ImGui.ColorButton("COLORBTN", col.red, col.green, col.blue, col.alpha, 70.0, 30.0)
        
        
        ImGui.Dummy(0, 30)
        if (ImGui.CollapsingHeader("Collapsing Header"))
        {
            if (ImGui.MenuItem("Menu Item1", false, true))
            {
                showTextBox("Menu Item Clicked")
            }
            ImGui.MenuItem("Menu Item2", false, true)
            ImGui.MenuItem("Menu Item3", true, true)

            if (ImGui.Button("Normal Button", 70.0, 30.0))
            {
                showTextBox("Button clicked")
            }
    
            if (ImGui.InvisibleButton("Invis Button", 70.0, 30.0))
            {
                showTextBox("Invis Button clicked")
            }
    
            ImGui.SameLine()
    
            if (ImGui.ColorButton("Color button", 0.25, 0.25, 0.25, 1.0, 0, 70.0, 30.0))
            {
                showTextBox("Color button clicked")
            }
        }
    
        ImGui.End()
        isOpen = windowValue.isOpen
    }  
    ImGui.EndFrame("MyWindow")  

    if (Pad.IsKeyPressed(VK_F5)) 
    {
        isOpen = !isOpen

        while (Pad.IsKeyPressed(VK_F5)) 
        {
            wait(10)
        }
    }
}


