pipeline {
    agent any
    
    parameters {
        string(name: 'REPO_NAME', defaultValue: 'rybmks/it-infrastructure-lab4', description: 'Repository name')
        string(name: 'REPO_URL', defaultValue: 'https://github.com/rybmks/it-infrastructure-lab4', description: 'Repository URL')
        string(name: 'BRANCH_NAME', defaultValue: 'dev', description: 'Full branch name')
    }
    
    stages {
        stage('Checkout') {
            steps {
                script {
                    echo "Cloning public repository: ${params.REPO_URL}"
                    git branch: params.BRANCH_NAME, url: params.REPO_URL
                }
            }
        }
        
        stage('Build') {
            steps {
                script {
                    sh 'gcc -o main main.c' 
                    if (!fileExists('./main')) {
                        error "main was not found." 
                    }
                }
            }
        }
        
        stage('Define Variant') {
            steps {
                script {
                    def output = sh(script: "echo dummy | ./main", returnStdout: true).trim() 
                    def outputLines = output.split('\n')
                    
                    def variantLine = outputLines[0].trim()
                    def variant = variantLine as Integer 
                    
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
                    
                    reportContent.append("<html><body><h2>Test report</h2><table border='1'>")
                    reportContent.append("<tr><th>Input</th><th>Expected</th><th>Output</th><th>Test status</th></tr>") 
                    
                    testCases.each { testCase ->
                        def testOutput = sh(script: "echo \"${testCase.input}\" | ./main", returnStdout: true).trim().split('\n') 
                        def result = testOutput.length > 1 ? testOutput[1].trim() : testOutput[0].trim()
                        
                        def passed = (result == testCase.expected) 
                        
                        reportContent.append("<tr>")
                        reportContent.append("<td>${testCase.input}</td>")
                        reportContent.append("<td>${testCase.expected}</td>")
                        reportContent.append("<td>${result}</td>")
                        
                        reportContent.append("<td style='color:${passed ? "green" : "red"}'>${passed ? "Passed" : "Failed"}</td>") 
                        reportContent.append("</tr>")
                        
                        if (!passed) {
                            reportContent.append("</table></body></html>")
                            writeFile file: 'report.html', text: reportContent.toString()
                            error "Test failed: was expected '${testCase.expected}', but got '${result}'"
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
                sh 'wkhtmltopdf report.html report.pdf' 
                
                emailext (
                    subject: "Test report for ${params.REPO_NAME}",
                    body: """<p>PDF Test report is attached.</p>
                             <p>Repository: ${params.REPO_NAME}</p>""",
                    to: "rybalkomaks3@gmail.com",
                    mimeType: 'text/html',
                    attachmentsPattern: "report.pdf"
                )
            }
        }
    }
}