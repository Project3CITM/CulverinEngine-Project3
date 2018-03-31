using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Return : CulverinBehaviour
{
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            //SceneManager.LoadScene("Menu");
        }
    }

    void OnClick()
    {
        SceneManager.LoadScene("Menu");
    }
}