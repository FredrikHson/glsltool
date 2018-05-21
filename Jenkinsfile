pipeline
{
    agent any
    stages
    {
        stage('build')
        {
            steps
            {
                sh 'make'
            }
        }
    }
    post
    {
        always
        {
            echo 'One way or another, I have finished'
            deleteDir() /* clean up our workspace */
        }
        success
        {
            echo 'I succeeeded!'
            slackSend color: 'good', message: "glsltool ${env.BRANCH_NAME} built successfully"
        }
        failure
        {
            slackSend color: 'bad', message: "glsltool ${env.BRANCH_NAME} failed to build ${env.BUILD_URL}"
        }
    }
