using CulverinEditor;
using CulverinEditor.Debug;


public class TestLog
{
    void Update()
    {   
        Debug.Log("Hello", Department.PLAYER, Color.BLUE);

        Debug.Log("Good Bye", Department.IA, Color.RED);

        Debug.Log("Y lo saee", Department.STAGE, Color.PINK);
    }
}

