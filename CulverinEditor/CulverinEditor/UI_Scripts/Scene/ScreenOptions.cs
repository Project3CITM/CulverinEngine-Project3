using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;
using CulverinEditor.Screen;

public class ScreenOptions : CulverinBehaviour
{
    public GameObject options_menu;
    public GameObject screenoptions_menu;
    public GameObject display_button;

    public GameObject windowed_cb;
    public GameObject fullscreen_cb;
    public GameObject bordeless_cb;

    public GameObject brightness_sl;//display
    public GameObject fov_sl;//audio

    public GameObject vsync_cb;
    public GameObject showfps_cb;
    public GameObject vibration_cb;

    void Start()
    {
        options_menu = GetLinkedObject("options_menu");
        screenoptions_menu = GetLinkedObject("screenoptions_menu");
        display_button = GetLinkedObject("display_button");

        windowed_cb = GetLinkedObject("windowed_cb");
        fullscreen_cb = GetLinkedObject("fullscreen_cb");
        bordeless_cb = GetLinkedObject("bordeless_cb");

        brightness_sl = GetLinkedObject("brightness_sl");
        fov_sl = GetLinkedObject("fov_sl");

        vsync_cb = GetLinkedObject("vsync_cb");
        showfps_cb = GetLinkedObject("showfps_cb");
        vibration_cb = GetLinkedObject("vibration_cb");
        screenoptions_menu.SetActive(false);
    }

    public void CloseScreenOptions()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        screenoptions_menu.SetActive(false);
        options_menu.SetActive(true);
        EventSystem.SendInteractiveSelected(display_button);
    }

    public void SetWindowed()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        fullscreen_cb.GetComponent<CompCheckBox>().HideTick();
        bordeless_cb.GetComponent<CompCheckBox>().HideTick();
        Screen.SetWindowed();
    }
    public void SetFullscreen()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        windowed_cb.GetComponent<CompCheckBox>().HideTick();
        bordeless_cb.GetComponent<CompCheckBox>().HideTick();
        Screen.SetFullScreen();
    }
    public void SetBorderless()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        windowed_cb.GetComponent<CompCheckBox>().HideTick();
        fullscreen_cb.GetComponent<CompCheckBox>().HideTick();
        Screen.SetBordeless();
    }
    public void SetVSync()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        Screen.SetBordeless();
    }
    public void ShowFPS()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        Screen.ShowFPS();
    }
    public void SwapControllerVibration()
    {
        GetComponent<CompAudio>().PlayEvent("Push");
        Screen.SwapControllerVibration();
    }

}
