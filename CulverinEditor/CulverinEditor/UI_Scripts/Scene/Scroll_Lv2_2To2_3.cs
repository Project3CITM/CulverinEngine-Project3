using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Scroll_Lv2_2To2_3 : CulverinBehaviour
{
    public GameObject scroll_text;
    public GameObject loading_text;
    public GameObject button;
    public float speed = 0.0f;
    public float limit_y = 0.0f;
    bool start_load = false;
    bool end_scroll = false;

    Vector3 position = Vector3.Zero;

    void Start()
    {
        scroll_text = GetLinkedObject("scroll_text");
        button = GetLinkedObject("button");
        loading_text = GetLinkedObject("loading_text");
        position = scroll_text.GetComponent<CompRectTransform>().GetUIPosition();
        button.SetActive(false);
        loading_text.SetActive(false);

    }
    void Update()
    {
        if (scroll_text.GetComponent<CompRectTransform>().GetUIPosition().y < limit_y)
        {
            position.y += speed * Time.deltaTime;
            scroll_text.GetComponent<CompRectTransform>().SetUIPosition(position);
        }
        if (!end_scroll && scroll_text.GetComponent<CompRectTransform>().GetUIPosition().y >= limit_y)
        {
            loading_text.SetActive(true);
            end_scroll = true;
            start_load = true;
            SceneManager.LoadNewWalkableMap("LVL2_Zone3");
            SceneManager.LoadMultiSceneNoDestroy("LVL2_zone_3", "PauseMenu");
        }
        if (start_load)
        {
            if (SceneManager.CheckMultiSceneReady())
            {
                start_load = false;
                loading_text.SetActive(false);
                button.SetActive(true);
            }
        }
    }


}