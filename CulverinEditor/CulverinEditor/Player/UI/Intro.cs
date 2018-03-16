using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Intro : CulverinBehaviour
{
    void OnClick()
    {
        SceneManager.LoadScene("NewScene");
    }
}