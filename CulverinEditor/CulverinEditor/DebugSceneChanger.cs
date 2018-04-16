using CulverinEditor;
using CulverinEditor.SceneManagement;


class DebugSceneChanger : CulverinBehaviour
{
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            SceneManager.LoadScene("BossTestScene");
        }
    }
}