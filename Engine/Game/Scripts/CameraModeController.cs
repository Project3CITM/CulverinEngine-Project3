using CulverinEditor;

public class CameraModeController : CulverinBehaviour
{
    TestMovement movement_script;

    void Start()
    {
        movement_script = GetComponent<TestMovement>();
    }

    public void EnableCombatMode()
    {
        movement_script.BlockCamera();
    }

    public void DisableCombatMode()
    {
        movement_script.UnBlockCamera();
    }
}
       