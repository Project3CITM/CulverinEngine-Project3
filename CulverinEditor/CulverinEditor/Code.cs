using CulverinEditor;
using CulverinEditor.Debug;

public class Code : CulverinBehaviour
{
    public int num = 0;
    public float num2 = 0.0f;
    void Update()
    {
        num++;
        num2 += 0.1f;
    }

    public void Hola()
    {

    }
    public void Hello(int number)
    {
        Debug.Log(number);
    }
    public void Hello2(float floa)
    {

    }
    public void Hello3(string str)
    {

    }
    public void Hello4(bool bos)
    {

    }
    public void Hello5(GameObject number)
    {

    }
}