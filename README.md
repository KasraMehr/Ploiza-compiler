void openImage(const char *imgFilename) {
#ifdef _WIN32
    // ویندوز: استفاده از دستور start
    char command[256];
    snprintf(command, sizeof(command), "start %s", imgFilename);
    system(command);
#elif defined(__APPLE__)
    // macOS: استفاده از دستور open
    char command[256];
    snprintf(command, sizeof(command), "open %s", imgFilename);
    system(command);
#else
    // لینوکس: استفاده از دستور xdg-open
    char command[256];
    snprintf(command, sizeof(command), "xdg-open %s", imgFilename);
    system(command);
#endif
}


void generateASTImage(ASTNode *root, const char *dotFilename, const char *imgFilename) {
    // ابتدا فایل DOT تولید میشه.
    generateASTDotFile(root, dotFilename);
    
    // ساخت دستور سیستم برای تبدیل فایل DOT به تصویر (اینجا PNG استفاده شده)
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dotFilename, imgFilename);
    
    // اجرای دستور سیستم
    int ret = system(command);
    if (ret != 0) {
        printf("Error: failed to execute Graphviz command.\n");
    } else {
        printf("AST image generated successfully: %s\n", imgFilename);
        openImage(imgFilename);
    }
    
}
