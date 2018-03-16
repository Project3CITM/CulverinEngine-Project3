using CulverinEditor;
using CulverinEditor.Debug;

public class Test_Input_New : CulverinBehaviour
{
    public virtual void Update()
    {

        float move = Input.GetInput_ControllerAxis("VerticalK", "Player");
        if(move > 0)
        {
            Debug.Log("Positive Button");
        }
        if (move < 0)
        {
            Debug.Log("Negative Button");
        }

    }

}