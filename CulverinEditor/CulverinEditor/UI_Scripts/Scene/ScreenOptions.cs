using CulverinEditor;
using CulverinEditor.EventSystem;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class ScreenOptions : CulverinBehaviour
{
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
        windowed_cb = GetLinkedObject("windowed_cb");
        fullscreen_cb = GetLinkedObject("fullscreen_cb");
        bordeless_cb = GetLinkedObject("bordeless_cb");

        brightness_sl = GetLinkedObject("brightness_sl");
        fov_sl = GetLinkedObject("fov_sl");

        vsync_cb = GetLinkedObject("vsync_cb");
        showfps_cb = GetLinkedObject("showfps_cb");
        vibration_cb = GetLinkedObject("vibration_cb");
    }

    public void SetWindowed()
    {
        fullscreen_cb.GetComponent<CompCheckBox>().HideTick();
        bordeless_cb.GetComponent<CompCheckBox>().HideTick();
    }
    public void SetFullscreen()
    {
        windowed_cb.GetComponent<CompCheckBox>().HideTick();
        bordeless_cb.GetComponent<CompCheckBox>().HideTick();
    }
    public void SetBorderless()
    {
        windowed_cb.GetComponent<CompCheckBox>().HideTick();
        fullscreen_cb.GetComponent<CompCheckBox>().HideTick();
    }

}
