using CulverinEditor;

public class Code : CulverinBehaviour
{
    void Start()
    {
        GetComponent<Transform>().enabled = false;
        gameObject.GetComponent<Transform>().enabled = true;
    }
}