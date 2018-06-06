using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;
using CulverinEditor.SceneManagement;

public class DamageFeedback : CulverinBehaviour
{
    Material mat;

    private bool damage = false;
    private float alpha = 1.0f;
    bool dead = false;
    bool reviving = false;
    float mult_dead = 0.0f;

    //Duration of the blood texture at the screen
    public float blood_time = 2.0f;
    public float dying_time = 2.0f;
    float curr_dead_time = 0.0f;
    public float fade_maintained_time = 5.0f;

    public GameObject fade_image;
    public GameObject total_fade_image;
    public GameObject you_died_image;

    bool total_dead = false;
    public float last_dying_timer = 5.0f;
    float last_dying_current_timer = 0.0f;
    float you_died_value = 0.0f;

    CharactersManager characters_manager;

    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        damage = false;
        dead = false;
        reviving = false;
        total_dead = false;
        alpha = 0.0f;
        curr_dead_time = 0.0f;
        mult_dead = 0.0f;
        you_died_value = 0.0f;
        fade_image = GetLinkedObject("fade_image");
        total_fade_image = GetLinkedObject("total_fade_image");
        you_died_image = GetLinkedObject("you_died_image");

        characters_manager = GetComponent<CharactersManager>();
    }

    void Update()
    {
        //Manage Blood Feedback Sprite --------------------
        if (alpha > 0.0f)
        {
            float decrease_val = Time.deltaTime / blood_time;
            alpha -= decrease_val;
        }

        if (alpha <= 0.0f)
        {
            damage = false;
        }
        //-------------------------------------------------

        //Manage Fade to Black ----------------------------
        if (dead)
        {
            if (mult_dead >= 0.0f)
            {
                float decrease_val = Time.deltaTime / dying_time;
                mult_dead += decrease_val;
                fade_image.GetComponent<CompImage>().SetAlpha(mult_dead);
            }

            if (mult_dead >= 1.0f)
            {
                mult_dead = 1.0f;
                dead = false;
                reviving = true;
            }
        }

        if (reviving)
        {
            curr_dead_time += Time.deltaTime;
            if (curr_dead_time > fade_maintained_time)
            {
                float increase_val = Time.deltaTime / dying_time;
                mult_dead -= increase_val;
                fade_image.GetComponent<CompImage>().SetAlpha(mult_dead);
            }
            if (mult_dead <= 0.0f)
            {
                mult_dead = 0.0f;
                fade_image.GetComponent<CompImage>().SetAlpha(0.0f);
                curr_dead_time = 0.0f;
                reviving = false;
                characters_manager.dying = false;
            }
        }
        //--------------------------------------------------

        mat.SetBool("damage", damage);
        mat.SetFloat("alpha", alpha);


        if(total_dead)
        {
            if (mult_dead >= 0.0f)
            {
                float decrease_val = Time.deltaTime / dying_time;
                mult_dead += decrease_val;
                total_fade_image.GetComponent<CompImage>().SetAlpha(mult_dead);
            }

            if (mult_dead >= 1.0f)
            {
                last_dying_current_timer += Time.deltaTime;
                float decrease_val = Time.deltaTime / dying_time;
                you_died_value += decrease_val;
                you_died_image.GetComponent<CompImage>().SetAlpha(you_died_value);
                if (last_dying_current_timer >= last_dying_timer && you_died_value >= 1.5f)
                {
                    if (SceneManager.CheckMultiSceneReady())
                    {
                        SceneManager.RemoveSecondaryScene();
                    }
                    SceneManager.LoadScene("LoseMenuV2");
                    StatsScore.SoftReset();
                    characters_manager.dying = false;
                }
            }
        }

    }

    //Depending on the hp left, set more alpha
    public void SetDamage(float curr_hp, float max_hp)
    {
        float result = curr_hp / max_hp;
        alpha = 1.0f - result * 0.5f;
        damage = true;
    }

    public void CharacterDie()
    {
        mult_dead = 0.0f;
        dead = true;
        curr_dead_time = 0.0f;
        reviving = false;
        characters_manager.dying = true;
    }

    public void TotalDefeat()
    {
        total_dead = true;
        characters_manager.dying = true;
    }
}