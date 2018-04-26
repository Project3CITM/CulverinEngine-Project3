using CulverinEditor;
using CulverinEditor.SceneManagement;
using CulverinEditor.Debug;

public class SwapScene : CulverinBehaviour
{
    public string sceneName = "MetricsTestScene";
    public int swapSceneIndexSuf = 0;

    private string nextSceneName = "";

    void Start()
    {
        nextSceneName = sceneName + swapSceneIndexSuf.ToString();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.K))
        {
            Debug.Log("Changing scene to: " + nextSceneName);
            SceneManager.LoadScene(nextSceneName);
        }
    }
}