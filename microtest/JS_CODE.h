EM_ASYNC_JS(void, init_offline_fs, (), {
  console.log('Check persistent exists.');
  if (!FS.analyzePath('/persistent').exists) {
	console.log('try create /persistent');
    FS.mkdir('/persistent');
	console.log('try mount /persistent as IDBFS');
    FS.mount(IDBFS, {autoPersist: true}, '/persistent');	
	console.log('try Promise IDBFS');
    await new Promise((resolve) => {
      FS.syncfs(true, err => { assert(!err); resolve(); });
    });
	console.log('end Promise IDBFS');		
  }
  console.log('Persistent check completed.');
});

EM_ASYNC_JS(void, sync_offline_fs, (), {
  console.log('try sync Promise IDBFS');
  await new Promise((resolve) => {
    FS.syncfs(false, err => { assert(!err); resolve(); });
  });
  console.log('end sync Promise IDBFS');
});

// Define a JS function callable from C
EM_JS(void, save_file_from_memfs_to_disk, (const char* memfs_name, const char* local_name), {
    saveFileFromMEMFSToDisk(UTF8ToString(memfs_name), UTF8ToString(local_name));
    });

EM_JS(void, open_file_dialog, (), {
  openFile();
    });

EM_JS(void, request_clipboard_paste, (), {
    pasteFromClipboard();
    });

EM_JS(void, copy_to_clipboard, (const char* text), {
    copyToClipboard(text);
    });
