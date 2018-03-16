using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Return : CulverinBehaviour
{


    void Start()
    {
        Input.SetInputManagerActive("GUI", true);
    }

    void OnClick()
    {
        SceneManager.LoadScene("Menu");
    }
}