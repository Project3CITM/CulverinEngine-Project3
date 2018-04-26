using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class MenuScript : CulverinBehaviour
{
    void Update()
    {
        if (Input.GetInput_KeyDown("Menu", "Player"))
        {
            if (SceneManager.CheckMultiSceneReady())
            {
                SceneManager.BlockGUIinput();
                SceneManager.ChangeToSecondaryScene();

            }
        }
    }
}