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
    public bool save_playerinfo = false;
    public bool load_playerinfo = false;
    public int scene_load = 0;
    bool fadeing = false;
    enum StateFade
    {
        GO_BLACK,
        GO_NORMAL,
        NONE
    }
    StateFade state = StateFade.NONE;

    enum SceneToLoad
    {
        TUTORIAL,
        ALPHA2_LEVEL_1_1,
        ALPHA2_LEVEL_1_2,
        ALPHA2_LEVEL_1_3,
        LVL2_ZONE1,
        LVL2_ZONE2,
        LVL2_ZONE3,
        NONE
    }
    SceneToLoad scene = SceneToLoad.NONE;

    void Start()
    {
        image_fade = GetLinkedObject("image_fade");
        saveinfo_player = GetLinkedObject("saveinfo_player");
        if (go_normal)
        {
            Time.timeScale = 0.0f;
            state = StateFade.GO_NORMAL;
            actual_alpha = 1.0f;
            image_fade.SetActive(true);
            image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
            go_normal = false;
            fadeing = true;
        }
        else
        {
            image_fade.SetActive(false);
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
                        actual_alpha += speed_fade * Time.deltaTimespecial;
                        break;
                    }
                case StateFade.GO_NORMAL:
                    {
                        actual_alpha -= speed_fade * Time.deltaTimespecial;
                        break;
                    }
            }
            if(actual_alpha > 1.0f)
            {
                actual_alpha = 1.0f;
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
                if (save_playerinfo)
                {
                    saveinfo_player.GetComponent<CharactersManager>().SaveInfoPlayer();
                }
                Time.timeScale = 1.0f;
                // Now Load Scene
                image_fade.SetActive(false);
                LoadScene();
            }
            else if(actual_alpha < 0.0f)
            {
                actual_alpha = 0.0f;
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
                fadeing = false;
                state = StateFade.NONE;
                Time.timeScale = 1.0f;
                image_fade.SetActive(false);
            }
            else
            {
                image_fade.GetComponent<CompImage>().SetAlpha(actual_alpha);
            }
        }
        if (load_playerinfo)
        {
            saveinfo_player.GetComponent<CharactersManager>().LoadInfoPlayer();
            load_playerinfo = false;
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
                image_fade.SetActive(true);
                Time.timeScale = 0.0f;
            }
        }
    }

    void LoadScene()
    {
        scene = (SceneToLoad)scene_load;
        switch (scene)
        {
            case SceneToLoad.TUTORIAL:
                {
                    //   SceneManager.LoadNewWalkableMap("Map_Level_Tutorial");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Tutorial Real Scene");
                    break;
                }
            case SceneToLoad.ALPHA2_LEVEL_1_1:
                {
                    //   SceneManager.LoadNewWalkableMap("Map_Level_1_1");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("TutorialToLvl1_1");
                    break;
                }
            case SceneToLoad.ALPHA2_LEVEL_1_2:
                {
                    //   SceneManager.LoadNewWalkableMap("Map_Level_1_2");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Lvl1_1ToLvl1_2");
                    break;
                }
            case SceneToLoad.ALPHA2_LEVEL_1_3:
                {
                    //   SceneManager.LoadNewWalkableMap("Map_Level_1_3");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Lvl1_2ToLvl1_3");
                    break;
                }
            case SceneToLoad.LVL2_ZONE1:
                {
                    //SceneManager.LoadNewWalkableMap("LVL2_Zone1");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Lvl1_3ToLvl2_1");
                    break;
                }
            case SceneToLoad.LVL2_ZONE2:
                {
                   // SceneManager.LoadNewWalkableMap("LVL2_Zone2");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Lvl2_1ToLvl2_2");
                    break;
                }
            case SceneToLoad.LVL2_ZONE3:
                {
                    //SceneManager.LoadNewWalkableMap("LVL2_Zone3");
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("Lvl2_2ToLvl2_3");
                    break;
                }
        }
    }
}