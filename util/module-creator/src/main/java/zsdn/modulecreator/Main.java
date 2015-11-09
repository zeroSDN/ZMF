package zsdn.modulecreator;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Iterator;

import org.apache.commons.io.FileUtils;

import com.beust.jcommander.JCommander;
import com.beust.jcommander.ParameterException;

/**
 * Copies a dir to another dir, replacing the name of the SourceDir in every file contained within, and renames the folder itself.
 * Used for copying a template module to modules/cpp, creating a new module with custom name and custom id.
 * 
 * @author Andre Kutzleb
 *
 */
public class Main {

	public static void main(String[] args) throws IOException, InterruptedException {
		
		
		try {
			ParameterDefinition jct = new ParameterDefinition();
			JCommander jCommander = new JCommander(jct);
				try {
				jCommander.parse(args);
				
				String templateName = new File(jct.templateFolder).getName();
				copyAndRename(jct.templateFolder, jct.targetFolder + File.separator + jct.moduleName, jct.moduleName,jct.moduleId,templateName);
				copyAndRename(jct.templateUnitestFolder, jct.targetFolder + File.separator + jct.moduleName +"-UT", jct.moduleName,jct.moduleId,templateName);

			} catch (ParameterException pe) {
				System.err.println(pe.getMessage());
				jCommander.usage();
				return;
			}

		} catch (Exception e) {
			System.err.println(String.format("Stopping execution due to unexpected exception: %s", e.getMessage()));
		}	
	}
	


	private static void copyAndRename(String src, String dst, String newModuleName, String modTypeId, String templateName) throws IOException {
		
		File srcDir = new File(src);
		File dstDir = new File(dst);
		
		if(!srcDir.isDirectory()) {
			throw new IllegalArgumentException(src + " is not a directory.");
		}
		

		FileUtils.copyDirectory(srcDir,dstDir);
		
		Iterator<File> iter = FileUtils.iterateFiles(dstDir,null, true);
		
		while(iter.hasNext()) {
			
			File current = iter.next();
			String content  = new String(Files.readAllBytes(current.toPath()));
			content = content.replace(templateName, newModuleName).replace("static const uint16_t MODULE_TYPE_ID = 0x0000;", "static const uint16_t MODULE_TYPE_ID = 0x" +modTypeId+ ";");
			Files.write(current.toPath(), content.getBytes());
			
			File renameTo = new File(current.getParent() + File.separator +  current.getName().replace(templateName, newModuleName));
			current.renameTo(renameTo);
		}		
	}
}
