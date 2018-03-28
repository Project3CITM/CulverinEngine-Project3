using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


class PKeyScene : CulverinBehaviour
{
    void Update()
    {
        if (Input.GetInput_KeyDown("TriangleR", "Player"))
        {
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("TriangleL", "Player"))
        {
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("RAllyAttack", "Player"))
        {
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("LAllyAttack", "Player"))
        {
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("RAttack", "Player"))
        {
            SceneSwap();
        }
        if (Input.GetInput_KeyDown("LAttack", "Player"))
        {
            SceneSwap();
        }
    }

    void SceneSwap()
    {
        Debug.Log("SceneSwap");
        SceneManager.LoadScene("MainMenuScene");
    }
}