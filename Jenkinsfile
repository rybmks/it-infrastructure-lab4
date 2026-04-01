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
                script {
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
                        env.testCasesJson = groovy.json.JsonOutput.toJson([
                            ['input': 'hello', 'expected': 'hElLo'],
                            ['input': '123456', 'expected': '123456'],
                            ['input': 'Hello world 123', 'expected': 'hElLo wOrLd 123'],
                            ['input': '*/1-==1', 'expected': '*/1-==1']
                        ])
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
                    def testCases = new groovy.json.JsonSlurper().parseText(env.testCasesJson)
                    
                    reportContent.append("<html><body style='font-family: sans-serif;'><h2>Test report</h2><table border='1' style='border-collapse: collapse; width: 100%;'>")
                    reportContent.append("<tr style='background-color: #f2f2f2;'><th>Input</th><th>Expected</th><th>Output</th><th>Status</th></tr>") 
                    
                    testCases.each { testCase ->
                        def testOutput = sh(script: "echo \"${testCase.input}\" | ./main", returnStdout: true).trim().split('\n') 
                        def result = testOutput.length > 1 ? testOutput[1].trim() : testOutput[0].trim()
                        def passed = (result == testCase.expected) 
                        
                        reportContent.append("<tr>")
                        reportContent.append("<td style='padding: 8px;'>${testCase.input}</td>")
                        reportContent.append("<td style='padding: 8px;'>${testCase.expected}</td>")
                        reportContent.append("<td style='padding: 8px;'>${result}</td>")
                        reportContent.append("<td style='padding: 8px; color:${passed ? "green" : "red"}'><b>${passed ? "Passed" : "Failed"}</b></td>") 
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
                if (fileExists('report.html')) {
                    def htmlBody = readFile 'report.html'
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