using CulverinEditor;
using CulverinEditor.SceneManagement;


class TestLoad : CulverinBehaviour
{
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Y))
        {
            SceneManager.LoadScene("TestScene");
        }
    }
}