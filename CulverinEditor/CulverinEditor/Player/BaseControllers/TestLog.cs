using CulverinEditor;
using CulverinEditor.Debug;


public class TestLog
{
    void Update()
    {   
        Debug.Log("Hello", Department.PLAYER, Color.BLUE);

        Debug.Log("Good Bye", Department.IA, Color.RED);

        Debug.Log("Y lo saee", Department.STAGE, Color.PINK);

        Debug.Log("My name is Jeff", Department.GENERAL, Color.NONE);

        Debug.Log("Collision", Department.PHYSICS, Color.YELLOW);
    }
}

