using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;


public class Scroll : CulverinBehaviour
{
    public GameObject scroll_text;
    CompText lore;
    public float speed = 0.0f;
    public float limit_y = 0.0f;
    Vector3 position = Vector3.Zero;
    
    void Start()
    {
        lore = scroll_text.GetComponent<CompText>();
        position = lore.GetRectTransform().GetPosition();
    }
    void Update()
    {
        Vector3 new_pos = new Vector3(position.x, position.y + speed * Time.deltaTime, position.z);
        lore.GetRectTransform().SetPosition(new_pos);
        if(lore.GetRectTransform().GetPosition().y> limit_y)
        {
            SceneManager.LoadScene("MainMenuScene");
        }
    }


}