using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class PKeyScene : CulverinBehaviour
{
    void Update()
    {
        Debug.Log("PressAnyKey");
        if (Input.GetInput_KeyDown("TriangleR", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("TriangleL", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("RAllyAttack", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("LAllyAttack", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
    }

    void SceneSwap()
    {
        Debug.Log("SceneSwap");
        SceneManager.LoadScene("MainMenuScene");
    }
}