using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.SceneManagement;


public class FadeToBlack : CulverinBehaviour
{
    float actual_alpha = 0.0f;
    public float speed_fade = 0.2f;
    public bool go_normal = false; //if this is true...
    public bool active_fade = false; //
    bool fadeing = false;
    enum StateFade
    {
        GO_BLACK,
        GO_NORMAL,
        NONE
    }
    StateFade state = StateFade.NONE;

    void Start()
    {
        if (go_normal)
        {
            state = StateFade.GO_NORMAL;
            actual_alpha = 1.0f;
            GetComponent<CompImage>().SetAlpha(actual_alpha);
            go_normal = false;
        }
    }

    void Update()
    {
        if(fadeing)
        {
            switch (state)
            {
                case StateFade.GO_BLACK:
                    {
                        actual_alpha += speed_fade * Time.deltaTime;
                        break;
                    }
                case StateFade.GO_NORMAL:
                    {
                        actual_alpha -= speed_fade * Time.deltaTime;
                        break;
                    }
            }
            if(actual_alpha > 1.0f)
            {
                actual_alpha = 1.0f;
                GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
                SceneManager.LoadNewWalkableMap("TestElliot");
                SceneManager.LoadScene("Level_3_v3");
            }
            else if(actual_alpha < 0.0f)
            {
                actual_alpha = 0.0f;
                GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
            }
            else
            {
                GetComponent<CompImage>().SetAlpha(actual_alpha);
            }
        }
    }

    void OnContact()
    {
        if (fadeing == false && active_fade)
        {
            CompCollider col = GetComponent<CompCollider>();
            GameObject obj_col = col.GetCollidedObject();

            if (obj_col != null && obj_col.CompareTag("player"))
            {
                Debug.Log("[yellow] Collision");
                //obj_col.GetComponent<>
                fadeing = true;
            }
        }
    }


}