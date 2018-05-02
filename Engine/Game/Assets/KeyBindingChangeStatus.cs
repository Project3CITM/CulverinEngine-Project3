using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class KeyBindingChangeStatus : CulverinBehaviour
{
    public float time_show = 5.0f;
    public float fade_speed = 0.1f;

    string text;
    bool show_message = false;
    bool start_fade = false;

    float actual_alpha = 1.0f;
    float current_time = 0.0f;


    void Update()
    {
        UpdateText();

        if (!Input.GetChangeInputActive())
            return;

        if (Input.GetChangeInputState() == 0)
        {
            SendMensage("Waiting for new key...");
        }
        else if (Input.GetChangeInputState() == 1)
        {
            SendMensage("Change key succes");

        }
        else if (Input.GetChangeInputState() == 2)
        {
            SendMensage("Change key fail");

        }
    }
    void SendMensage(string value)
    {
        GetComponent<CompText>().SetAlpha(1.0f);
        GetComponent<CompText>().SetText(value);
        show_message = true;

    }
    void UpdateText()
    {
        if (show_message && !start_fade)
        {
            current_time += Time.deltaTime;
            if (current_time > time_show)
            {
                start_fade = true;

            }

        }
        if (start_fade)
        {
            actual_alpha -= fade_speed * Time.deltaTime;
            if (actual_alpha<0.0f)
            {
                actual_alpha = 0.0f;
                start_fade = false;
            }
            GetComponent<CompText>().SetAlpha(actual_alpha);       
        }
    }
}