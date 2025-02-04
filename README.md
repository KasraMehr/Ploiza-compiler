void generateASTImage(ASTNode *root, const char *dotFilename, const char *imgFilename) {
    // ابتدا فایل DOT تولید میشه.
    generateASTDotFile(root, dotFilename);
    
    // ساخت دستور سیستم برای تبدیل فایل DOT به تصویر (اینجا PNG استفاده شده)
    char commandGenerate[256];
    snprintf(commandGenerate, sizeof(commandGenerate), "dot -Tpng %s -o %s", dotFilename, imgFilename);
    
    // اجرای دستور سیستم
    int ret = system(commandGenerate);
    if (ret != 0) {
        printf("Error: failed to execute Graphviz command.\n");
    } else {
        printf("AST image generated successfully: %s\n", imgFilename);
        char commandImage[256];
        snprintf(commandImage, sizeof(commandImage), "start %s", imgFilename);
        system(commandImage);
    }
    
}
