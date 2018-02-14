using CulverinEditor;
using CulverinEditor.Debug;

//Attach this script to the tank parent object if you want to see it rotate
public class Testelliot
{
    public float a = 5.0f;
    public float b = 6.0f;
    public Test test;

    void Start()
    {

    }

    void Update()
    {
        if (Input.KeyDown("S"))
        {
            test.PrintName();
        }
    }

    public float AwesomeNumber()
    {
        return a + b;
    }
}