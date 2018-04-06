using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class FadeAlpha : CulverinBehaviour
{
    public GameObject text;
    float actual_alpha = 1;
    bool increase = false;
    public float speed = 0.01f;

    void Start()
    {
        text = GetLinkedObject("text");
    }
    void Update()
    {

       if(increase == false)
        {
            actual_alpha-=speed*Time.deltaTime;
        }
        else if (increase)
        {
            actual_alpha += speed * Time.deltaTime;
        }
        if (actual_alpha <= 0)
        {
            increase = true;
            actual_alpha = 0;
        }
        else if (actual_alpha > 1)
        {
            increase = false;
            actual_alpha = 1;
        }
        text.GetComponent<CompText>().SetAlpha(actual_alpha);
    }

}