def testCases = []

pipeline {
    agent any
    
    parameters {
        string(name: 'REPO_NAME', defaultValue: 'rybmks/it-infrastructure-lab4')
        string(name: 'REPO_URL', defaultValue: 'https://github.com/rybmks/it-infrastructure-lab4')
        string(name: 'BRANCH_NAME', defaultValue: 'dev')
    }
    
    stages {
        stage('Checkout') {
            steps {
                git branch: params.BRANCH_NAME, url: params.REPO_URL
            }
        }
        
        stage('Build') {
            steps {
                sh 'gcc -o main main.c' 
                script {
                    if (!fileExists('./main')) { error "main was not found." }
                }
            }
        }
        
        stage('Define Variant') {
            steps {
                script {
                    def output = sh(script: "echo dummy | ./main", returnStdout: true).trim() 
                    def variant = output.split('\n')[0].trim() as Integer 
                    
                    if (variant == 15) { 
                        testCases = [
                            ['input': 'hello', 'expected': 'hElLo'],
                            ['input': '123456', 'expected': '123456'],
                            ['input': 'Hello world 123', 'expected': 'hElLo wOrLd 123'],
                            ['input': '*/1-==1', 'expected': '*/1-==1']
                        ]
                    } else {
                        error "Unsupported variant: ${variant}" 
                    }
                }
            }
        }
        
        stage('Test') {
            steps {
                script {
                    def reportContent = new StringBuilder()
                    reportContent.append("<html><body style='font-family: sans-serif;'><h2>Test report</h2><table border='1' style='border-collapse: collapse; width: 100%;'>")
                    reportContent.append("<tr style='background-color: #f2f2f2;'><th>Input</th><th>Expected</th><th>Output</th><th>Status</th></tr>") 
                    
                    testCases.each { testCase ->
                        def testOutput = sh(script: "echo \"${testCase.input}\" | ./main", returnStdout: true).trim().split('\n') 
                        def result = testOutput.length > 1 ? testOutput[1].trim() : testOutput[0].trim()
                        def passed = (result == testCase.expected) 
                        
                        reportContent.append("<tr><td style='padding: 8px;'>${testCase.input}</td><td style='padding: 8px;'>${testCase.expected}</td><td style='padding: 8px;'>${result}</td><td style='padding: 8px; color:${passed ? "green" : "red"}'><b>${passed ? "Passed" : "Failed"}</b></td></tr>")
                        
                        if (!passed) {
                            writeFile file: 'report.html', text: reportContent.toString() + "</table></body></html>"
                            error "Test failed: expected '${testCase.expected}', got '${result}'"
                        }
                    }
                    reportContent.append("</table></body></html>")
                    writeFile file: 'report.html', text: reportContent.toString()
                }
            }
        }
    }
    
    post {
        always {
            publishHTML([
                allowMissing: false,
                alwaysLinkToLastBuild: true,
                keepAll: true,
                reportDir: '.',
                reportFiles: 'report.html',
                reportName: 'Test Report'
            ]) 
            
            script {
                if (fileExists('report.html')) {
                    def htmlBody = readFile 'report.html'
                    catchError(buildResult: 'SUCCESS', stageResult: 'FAILURE') {
                        emailext (
                            subject: "CI/CD Report: ${params.REPO_NAME} #${env.BUILD_NUMBER}",
                            body: htmlBody,
                            to: "rybalkomaks3@gmail.com",
                            mimeType: 'text/html'
                        )
                    }
                }
            }
        }
    }
}