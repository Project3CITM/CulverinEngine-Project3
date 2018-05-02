using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.SceneManagement;


public class FadeToBlack : CulverinBehaviour
{
    public GameObject image_fade;
    public GameObject saveinfo_player;
    float actual_alpha = 0.0f;
    public float speed_fade = 0.2f;
    public bool go_normal = false; //if this is true...
    public bool active_fade = false; //
    public bool save_soad_info = false;
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
        image_fade = GetLinkedObject("image_fade");
        saveinfo_player = GetLinkedObject("saveinfo_player");
        if (go_normal)
        {
            state = StateFade.GO_NORMAL;
            actual_alpha = 1.0f;
            image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
            if (save_soad_info)
            {
                saveinfo_player.GetComponent<CharactersManager>().LoadInfoPlayer();
            }
            go_normal = false;
            fadeing = true;
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
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
                if (save_soad_info)
                {
                    saveinfo_player.GetComponent<CharactersManager>().SaveInfoPlayer();
                }

                SceneManager.LoadNewWalkableMap("TestElliot");
                SceneManager.LoadScene("Level_3_v3_elliot");
            }
            else if(actual_alpha < 0.0f)
            {
                actual_alpha = 0.0f;
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
            }
            else
            {
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
            }
        }
    }

    void OnTriggerEnter()
    {
        Debug.Log("Collision");
        if (fadeing == false && active_fade)
        {
            Debug.Log("Collision2");
            CompCollider col = GetComponent<CompCollider>();
            GameObject obj_col = col.GetCollidedObject();

            if (obj_col != null && obj_col.CompareTag("player"))
            {
                Debug.Log("Collision3");
                //obj_col.GetComponent<>
                state = StateFade.GO_BLACK;
                fadeing = true;
            }
        }
    }


}