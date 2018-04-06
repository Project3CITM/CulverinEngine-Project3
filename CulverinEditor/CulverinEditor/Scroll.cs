using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Scroll : CulverinBehaviour
{
    public GameObject scroll_text;
    public GameObject button;
    public float speed = 0.0f;
    public float limit_y = 0.0f;
    Vector3 position = Vector3.Zero;
    
    void Start()
    {
        scroll_text = GetLinkedObject("scroll_text");
        button = GetLinkedObject("button");
        position = scroll_text.GetComponent<CompRectTransform>().GetPosition();
        button.SetActive(false);
    }
    void Update()
    {
        if (scroll_text.GetComponent<CompRectTransform>().GetPosition().y < limit_y)
        {
            Debug.Log(position.y);
            Vector3 new_pos = new Vector3(position.x, position.y + speed * Time.deltaTime, position.z);
            scroll_text.GetComponent<CompRectTransform>().SetPosition(new_pos);
        }
        if (scroll_text.GetComponent<CompRectTransform>().GetPosition().y >= limit_y)
        {
            Debug.Log("MY NAME IS JEFFFFFFFFFFF");
            button.SetActive(true);

        }
        //SceneManager.LoadScene("MainMenuScene");
    }


}