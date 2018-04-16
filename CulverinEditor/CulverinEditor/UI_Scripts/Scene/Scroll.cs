using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Scroll : CulverinBehaviour
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
        position = scroll_text.GetComponent<CompRectTransform>().GetPosition();
        button.SetActive(false);
        loading_text.SetActive(false);

    }
    void Update()
    {
        if (scroll_text.GetComponent<CompRectTransform>().GetPosition().y < limit_y)
        {
            Vector3 new_pos = new Vector3(position.x, position.y + speed * Time.deltaTime, position.z);
            scroll_text.GetComponent<CompRectTransform>().SetPosition(new_pos);
        }
        if (!end_scroll && scroll_text.GetComponent<CompRectTransform>().GetPosition().y >= limit_y)
        {
            loading_text.SetActive(true);
            end_scroll = true;
            start_load = true;
            SceneManager.LoadNewWalkableMap("Map_Level_2");
            SceneManager.LoadSceneNoDestroy("Level 2");
        }
        if (start_load)
        {
            if (SceneManager.CheckSceneReady())
            {
                start_load = false;
                loading_text.SetActive(false);
                button.SetActive(true);
            }
        }
        //SceneManager.LoadScene("MainMenuScene");
    }


}