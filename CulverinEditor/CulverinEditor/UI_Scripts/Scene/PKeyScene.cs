using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class PKeyScene : CulverinBehaviour
{
    void Start()
    {
        Input.SetInputManagerActive("GUI", false);
    }
    void Update()
    {
        if(Input.GetInput_ControllerAxis("Vertical", "Player") > 0.5 || Input.GetInput_ControllerAxis("Vertical", "Player") < -0.5 )
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
        if (Input.GetInput_ControllerAxis("Horizontal", "Player") > 0.5 || Input.GetInput_ControllerAxis("Horizontal", "Player") < -0.5)
        {
            Debug.Log("SceneSwap");
            SceneSwap();
        }
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
        Input.SetInputManagerActive("GUI", true);
        SceneManager.LoadScene("MainMenuScene");
    }
}