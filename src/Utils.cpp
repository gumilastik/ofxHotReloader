#include "Utils.h"

void Utils::clearSubdirectories(std::string path) {
#if defined(JUCE_APP_VERSION)
	juce::Array<juce::File> files = juce::File(path).findChildFiles(File::TypesOfFileToFind::findFilesAndDirectories, true);
	for (size_t i = 0; i < files.size(); i++) {
		if (files[i].isDirectory()) {
			files[i].deleteRecursively();
		}
	}
#else
	ofDirectory dir = ofDirectory(path);
	std::vector<ofFile> files = dir.getFiles();
	for (size_t i = 0; i < files.size(); i++) {
		if (ofDirectory(files[i]).isDirectory()) {
			ofDirectory::removeDirectory(files[i].getAbsolutePath(), true, false);
		}
	}
#endif
}

void Utils::createDirectory(std::string pathDir) {
#if defined(JUCE_APP_VERSION)
	juce::File file(pathDir);
	if (!file.exists()) file.createDirectory();
#else
	ofDirectory dir(pathDir);
	if (!dir.exists()) dir.create(true);
#endif
}

std::string Utils::getDirectoryPath(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File::getCurrentWorkingDirectory().getChildFile(path).getFullPathName().toStdString();
#else
	return ofToDataPath(path, true);
#endif
}

bool Utils::checkFileExits(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path).exists();
#else
	return ofFile(path).exists();
#endif
}

bool Utils::copyFileFromTo(std::string from, std::string to) {
#if defined(JUCE_APP_VERSION)
	return juce::File(from).copyFileTo(juce::File(to));
#else
	return ofFile::copyFromTo(from, to, false, true);
#endif
}

std::string Utils::joinFilePath(std::string path1, std::string path2) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path1).getChildFile(path2).getFullPathName().toStdString();
#else
	return ofFilePath::join(path1, path2);
#endif
}

std::string Utils::getFileAbsolutePath(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path).getFullPathName().toStdString();
#else
	return ofFilePath::getAbsolutePath(path, true);
#endif
}

std::string Utils::getFileName(std::string path) {
#if defined(JUCE_APP_VERSION)
    return juce::File(path).getFileName().toStdString();
#else
    return ofFilePath::getFileName(path);
#endif
}

std::string Utils::getFileExtension(std::string path) {
#if defined(JUCE_APP_VERSION)
    return juce::File(path).getFileExtension().toStdString();
#else
    return ofFilePath::getFileExt(path);
#endif
}

std::string Utils::getEnclosingDirectory(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path).getParentDirectory().getFullPathName().toStdString();
#else
	return ofFilePath::getEnclosingDirectory(path);
#endif
}

std::string Utils::getFileBaseName(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path).getFileNameWithoutExtension().toStdString();
#else
	return ofFilePath::getBaseName(path);
#endif
}

unsigned long long Utils::getFileModificationTime(std::string path) {
#if defined(JUCE_APP_VERSION)
	return juce::File(path).getLastModificationTime().toMilliseconds();
#else
	return std::filesystem::last_write_time(ofFile(path));
#endif
}

unsigned long long Utils::getElapsedTimeMillis() {
#if defined(JUCE_APP_VERSION)
	return juce::Time::currentTimeMillis();
#else
	return ofGetElapsedTimeMillis();
#endif
}

void Utils::Execute(std::string command) {
#if defined(_WIN32)
	ShellExecuteA(NULL, NULL, command.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#elif defined(__APPLE__)
    system(command.c_str());
#endif
}
