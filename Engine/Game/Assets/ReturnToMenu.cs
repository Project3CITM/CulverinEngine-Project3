using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class ReturnToMenu : CulverinBehaviour
{

    void Start()
    {
        Input.SetInputManagerActive("GUI", true);

    }
    public void Return()
    {
        Input.SetInputManagerActive("GUI", false);
        SceneManager.LoadScene("MainMenuScene");
    }

}
