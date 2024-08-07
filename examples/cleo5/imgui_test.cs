                 8"cleo\example.jpg    "IMGUI_CLEO_DEMOG"  �  9   M ����"  �C  D"ImGuiCleo Demo Window        F" E" D"	 +"�  9  M �����
  )"�

 	ImGui: %d  "
 �

 Framerate: %d	  "
 *"�

 CLEO: %d  "
 )"+""WindowChild"TabBar!Checkbox,Button,Input,Text,Styles �  9   M B���+")""Show version info  *""Show timer example  )"+"�  9  M 4���+"�  	"Arrow buttonsM ����"Left '""Right'""Up'""Down,"+"�  	"Radio buttonsM ����""RB1  '"""RB2  '"""RB3  +"�  	"Regular buttonsM 4���M"S0   �  "Normal button  M "����
Normal button was pressed�'"�  "Image Button   M �����
Image Button was pressed�+"M"S1   �  "CB 1  �?          �?  M }����
Color button 1 was pressed�'"�  "CB 2      �?      �?  M -����
Color button 2 was pressed�'"�  "CB 3          �?  �?  M �����
Color button 3 was pressed�M"S2   "Tip:\n\tClick the below area�  "Invis button  M i����
Invisible button was pressed��  2"HOverM 8���"Hovering invisible button+","+"�  9  M J���+""	Int input� �  "Float input�̋B      �B +""
Int slider 
 "Float slider  �@       A +"�  !"Text input widget +"M"S012   �  "Rest Inputs  M 5���3"	Int input�3"
Int slider'"�  "Rest Sliders  M ����4"Float input�̋B4"Float slider  �@&"      �A            #"Color Picker    "COLORBTN      �B  �A&"       A" "ComboOption1,Option2,Option3  �  9  M ����+""*A little brown fox jumps over the lazy dog"*A little brown fox jumps over the lazy dog"*A little brown fox jumps over the lazy dog  �?          �?"*A little brown fox jumps over the lazy dog+""�A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog�  9  M ����+""Red checkbox+"<"�   � <"	Z  � <"x  � "Show version info  >"+""Button with custom style+":"   AM"S012   �  "Rounded button  M �����
Rounded button was pressed�="'"�  "Regular button  M �����
Regular button was pressed�"""�
    �      �M `����
Timer test message��    �  �
tM ����
tM ;���   Q����  9   M ���     ���    ����FLAG   SRC �"  //
// ImGuiCleo Example Script
//
{$CLEO}
{$USE CLEO}
{$USE imgui}

const TOGGLE_KEY = 116 // F5
int iShowWindow = 0
int iVersionInfo = 0
int iRadioBtn = 1
int iTimerExample = 0
int iPrevTimer = 0
int iComboSelection = 1
int iImage = ImGui.LoadImage("cleo\example.jpg")

while true
    /*
        This must always be 0 to render ImGui each frame
        If you need to wait for something use timers instead,
        example below
    */
    wait 0


    /*
        All ImGui code must be wrapped inside the BeginFrame() .. EndFrame()
        You need to pass a "UNIQUE ID" for the frame name.
        Use something that's unlikely to be used by any other scripts
    */
    ImGui.BeginFrame("IMGUI_CLEO_DEMO")
    ImGui.SetCursorVisible(iShowWindow)

    if iShowWindow == 1
    then
        ImGui.SetNextWindowSize(350.0, 600.0, ImGuiCond.Once)
        iShowWindow = ImGui.Begin("ImGuiCleo Demo Window", iShowWindow, 0, 0, 0, 0)

        int iReduxVer = ImGui.GetPluginVersion()
        int iImguiVer = ImGui.GetVersion()
        int iFps = Game.GetFramerate()

        ImGui.Spacing()

        if iVersionInfo == 1
        then
            LongString sTemp = ""
            ImGui.Columns(2)
            Text.StringFormat(sTemp, "ImGui: %d", iImguiVer)
            ImGui.Text(sTemp)
            Text.StringFormat(sTemp, "Framerate: %d", iFps)
            ImGui.Text(sTemp)
            ImGui.NextColumn()
            Text.StringFormat(sTemp, "CLEO: %d", iReduxVer)
            ImGui.Text(sTemp)
            ImGui.Columns(1)
            ImGui.Spacing()
        end

        ImGui.BeginChild("WindowChild")

        int iTab = ImGui.Tabs("TabBar", "Checkbox,Button,Input,Text,Styles")

        if iTab == 0
        then
            ImGui.Spacing()
            ImGui.Columns(2)
            iVersionInfo = ImGui.Checkbox("Show version info", iVersionInfo)
            ImGui.NextColumn()
            iTimerExample = ImGui.Checkbox("Show timer example", iTimerExample)
            ImGui.Columns(1)
            ImGui.Spacing()
        end


        if iTab == 1
        then
            ImGui.Spacing()
            if ImGui.CollapsingHeader("Arrow buttons")
            then
                ImGui.ButtonArrow("Left", 0)
                ImGui.SameLine()
                ImGui.ButtonArrow("Right", 1)
                ImGui.SameLine()
                ImGui.ButtonArrow("Up", 2)
                ImGui.SameLine()
                ImGui.ButtonArrow("Down", 3)

                ImGui.Separator()
                ImGui.Spacing()
            end
            
            if ImGui.CollapsingHeader("Radio buttons")
            then
                iRadioBtn = ImGui.RadioButton("RB1", iRadioBtn, 1)
                ImGui.SameLine()
                iRadioBtn = ImGui.RadioButton("RB2", iRadioBtn, 2)
                ImGui.SameLine()
                iRadioBtn = ImGui.RadioButton("RB3", iRadioBtn, 3)
                ImGui.Spacing()
            end

            if ImGui.CollapsingHeader("Regular buttons")
            then
                float fSzX, fSzY
                ImGui.GetScalingSize("S0", 2, 0, fSzX, fSzY)
                if ImGui.Button("Normal button", fSzX, fSzY)
                then
                    Text.PrintString("Normal button was pressed", 1000)
                end
                ImGui.SameLine()
                if ImGui.ButtonImage("Image Button", iImage, fSzX, fSzY)
                then
                    Text.PrintString("Image Button was pressed", 1000)
                end

                ImGui.Spacing()

                fSzX, fSzY = ImGui.GetScalingSize("S1", 3, 0)
                if ImGui.ButtonColored("CB 1", 1.0, 0.0, 0.0, 1.0, fSzX, fSzY)
                then
                    Text.PrintString("Color button 1 was pressed", 1000)
                end
                ImGui.SameLine()
                if ImGui.ButtonColored("CB 2", 0.0, 1.0, 0.0, 1.0, fSzX, fSzY)
                then
                    Text.PrintString("Color button 2 was pressed", 1000)
                end
                ImGui.SameLine()
                if ImGui.ButtonColored("CB 3", 0.0, 0.0, 1.0, 1.0, fSzX, fSzY)
                then
                    Text.PrintString("Color button 3 was pressed", 1000)
                end

                ImGui.GetScalingSize("S2", 1, 0, fSzX, fSzY)
                ImGui.Text("Tip:\n\tClick the below area")
                if ImGui.ButtonInvisible("Invis button", fSzX, fSzY)
                then
                    Text.PrintString("Invisible button was pressed", 1000)
                end

                if ImGui.IsItemHovered("HOver")
                then
                    ImGui.Text("Hovering invisible button")
                end

                ImGui.Spacing()
                ImGui.Separator()
            end
        end

        ImGui.Spacing()

        // Input and Slider example
        if iTab == 2
        then
            ImGui.Spacing()
            1@ = ImGui.InputInt("Int input", 420, 0, 1000)
            1@ = ImGui.InputFloat("Float input", 69.9, 0.0, 100.0)
            ImGui.Spacing()
            1@ = ImGui.SliderInt("Int slider", 5, 0, 10)
            1@ = ImGui.SliderFloat("Float slider", 5.0, 0.0, 10.0)
            ImGui.Spacing()
            LongString sText = ""
            
            // Ideally you'd want to allocate buffer
            sText = ImGui.InputText("Text input widget")
            ImGui.Spacing()

            fSzX, fSzY = ImGui.GetScalingSize("S012", 2, 0)
            if ImGui.Button("Rest Inputs", fSzX, fSzY)
            then
                ImGui.SetItemValueInt("Int input", 420)
                ImGui.SetItemValueInt("Int slider", 5)
            end
            ImGui.SameLine()
            if ImGui.Button("Rest Sliders", fSzX, fSzY)
            then
                ImGui.SetItemValueFloat("Float input", 69.9)
                ImGui.SetItemValueFloat("Float slider", 5.0)
            end

            ImGui.Dummy(0.0, 20.0)
            
            float r = 0
            float g = 0
            float b = 0
            float a = 0
            r, g, b, a = ImGui.ColorPicker("Color Picker")
            ImGui.ButtonColored("COLORBTN", r, g, b, a, 70.0, 30.0)

            ImGui.Dummy(0.0, 10.0)
            ImGui.Text(sText)

            iComboSelection = ImGui.ComboBox("Combo", "Option1,Option2,Option3", iComboSelection)
        end

        if iTab == 3
        then
            ImGui.Spacing()
            ImGui.Text("A little brown fox jumps over the lazy dog")
            ImGui.TextDisabled("A little brown fox jumps over the lazy dog")
            ImGui.TextColored("A little brown fox jumps over the lazy dog", 1.0, 0.0, 0.0, 1.0)
            ImGui.TextWithBullet("A little brown fox jumps over the lazy dog")
            ImGui.Spacing()
            ImGui.TextWrapped("A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog. A little brown fox jumps over the lazy dog")
        end

        if iTab == 4
        then
            ImGui.Spacing()
            ImGui.Text("Red checkbox")
            ImGui.Spacing()
            ImGui.PushStyleColor(ImGuiCol.FrameBg, 200, 0, 0, 255)
            ImGui.PushStyleColor(ImGuiCol.FrameBgActive, 90, 0, 0, 255)
            ImGui.PushStyleColor(ImGuiCol.FrameBgHovered, 120, 0, 0, 255)
            iVersionInfo = ImGui.Checkbox("Show version info", iVersionInfo)
            ImGui.PopStyleColor(3)
            ImGui.Spacing()
            ImGui.Text("Button with custom style")
            ImGui.Spacing()

            ImGui.PushStyleVar(ImGuiStyleVar.FrameRounding, 10.0)

            fSzX, fSzY = ImGui.GetScalingSize("S012", 2, 0)
            if ImGui.Button("Rounded button", fSzX, fSzY)
            then
                Text.PrintString("Rounded button was pressed", 1000)
            end
            ImGui.PopStyleVar(1)

            ImGui.SameLine()
            if ImGui.Button("Regular button", fSzX, fSzY)
            then
                Text.PrintString("Regular button was pressed", 1000)
            end
        end

        ImGui.EndChild()

        ImGui.End()
    end
    ImGui.EndFrame()


    // Timer example
    // This code has 5 second delay
    float fDelta = TIMERA - iPrevTimer
    if and
        iTimerExample > 0
        fDelta > 5000
    then
        Text.PrintString("Timer test message", 1000)
        iPrevTimer = TIMERA
    end

    if Pad.IsKeyPressed(TOGGLE_KEY)
    then
        while Pad.IsKeyPressed(TOGGLE_KEY)
            wait 0
        end
        
        if iShowWindow == 1
        then
           iShowWindow = 0
        else
            iShowWindow = 1
        end
    end
end


�	  __SBFTR 