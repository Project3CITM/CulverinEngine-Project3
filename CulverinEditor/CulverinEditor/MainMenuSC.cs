using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


class MainMenuScene : CulverinBehaviour
{
    void Update()
    {

    }

    void OnClick()
    {
        Debug.Log("SceneSwapGame");
        SceneManager.LoadScene("NewScene");
    }
}