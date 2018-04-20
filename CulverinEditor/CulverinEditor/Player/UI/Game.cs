using CulverinEditor;
using CulverinEditor.SceneManagement;

public class Game : CulverinBehaviour
{
    void Update()
    {
        
        if(Input.GetKeyDown(KeyCode.Num8))
        {
            SceneManager.LoadScene("WinScreen");
        }

        else if (Input.GetKeyDown(KeyCode.Num9))
        {
            SceneManager.LoadScene("LoseScreen");
        }
        
    }
}