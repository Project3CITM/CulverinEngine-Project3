using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Intro : CulverinBehaviour
{
    void Start()
    {
        Input.SetInputManagerActive("GUI", true);
    }

    void OnClick()
    {
        SceneManager.LoadScene("NewScene");
    }
}