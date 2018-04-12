using CulverinEditor;
using CulverinEditor.Debug;

public class Code : CulverinBehaviour
{

    void Update()
    {

        if (Input.GetKeyDown(KeyCode.A))
        {
            Instantiate("ss", 5.0f);
            //Destroy(, 23.0f);
        }
    }
}