using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class PKeyScene : CulverinBehaviour
{
    void Start()
    {
        Input.SetInputManagerActive("GUI", false);
        GetComponent<CompAudio>().PlayEvent("PlayUIMusic");
    }
    void Update()
    {
        if(Input.GetPressAnyKey())
        { 
            Debug.Log("SceneSwap");
            SceneSwap();
        }
    }

    void SceneSwap()
    {
        Debug.Log("SceneSwap");
        Input.SetInputManagerActive("GUI", true);
        SceneManager.LoadScene("MainMenuScene");
    }
}