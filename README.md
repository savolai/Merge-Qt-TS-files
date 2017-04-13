# Merge Qt .ts files

![Screenshot: This app allows you to combine multiple Qt .ts files into one](https://raw.githubusercontent.com/savolai/Merge-Qt-TS-files/master/screenshot.png)

This is a Qt project. The easiest way to use it is to install Qt, [download](https://github.com/savolai/Merge-Qt-TS-files/archive/master.zip) and open the project / CombineTranslationFiles.pro in Qt Creator, and then compile and run the project.

For use as-is. Made for my own use. Pull requests welcome.

## Usage: 
1. Open each source file by pressing "Browse source...". After doing that, click "Read in current source file translations". You will get a count of translations loaded into memory.
2. Repeat step 1 for each file you want to combine
3. After reading into memory all your translation data, select target file by pressing "Browse target...". After selecting filename to save to, press "Write translations to file". 

This app is designed to preserve as much as possible of the information in the translation files. 

When combining multiple translation files, this makes sure translations with empty content never override translations with existing content. 

If multiple of your source files contain conflicting, nonempty, translations for the same translation units (i.e classname+source text content), make sure to load the file that has the translations you want, last.

Caveat: This tool does NOT currently include multiple <location> elements if many exist in source translation units, only one of them is included for each translation. Mostly this never makes any difference, since a single run of lupdate project1.pro project2.pro -ts projects.ts (where projects.ts is the output file of this program) will put all the additional locations back again. 

## Background

This is useful when you have multiple applications

1. that have language files of their own
2. that statically link to common libraries that also have translations
    
In this situation, overlaps in translation files can cause strings not getting translated in the UI.
Also, qt docs don't exactly wanr about this situation, and also it tends to creep up on you; I found myself in a situation with hundreds of translation spread across multiple translation files that should have been one. So I wrote this to fix it.

The solution is to produce a common translation file for all the apps. First run your ts files through this app, combining translations. Then run lupdate like below - this updates the location tags of the translations to point to the right place(s).
After that, you can normally run lrelease on your ts file to produce .qm and load it from all your apps.

Thanks David Faure / @dfaure-kdab for pointing me to the right direction.

Other options that might help: 
1. When loading translation files into your app, first load the app specific language file and then on top of that, the translations for the common components. This occured to me too late. \o/ however, having all project translations in a single file tends to be a cleaner solution overall, as you will always know what has already been translated.
2. Use an XML merging tool such as those mentioned [in this StackOverflow post](http://stackoverflow.com/questions/80609/merge-xml-documents) might also work, but now we have a GUI :)
