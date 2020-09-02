void RocksDBRepositoryImpl::backup(void) {
  TI_INFO("start backup current db");

  rocksdb::BackupEngine* dummyBE;
  rocksdb::Status s = rocksdb::BackupEngine::Open(
      rocksdb::Env::Default(),
      rocksdb::BackupableDBOptions(db_backup_file_, (rocksdb::Env*)nullptr,
                                   share_table_files_),
      &dummyBE);

  if (!s.ok()) {
    TI_ERROR("backup open failed： %s", s.ToString().c_str());
    return;
  }

  std::unique_ptr<rocksdb::BackupEngine> backup_engine(dummyBE);
  {
    std::shared_lock<std::shared_mutex> lock(mutex_, std::try_to_lock);
    if (lock.owns_lock()) {
      s = backup_engine->CreateNewBackup(db_instance_.get(), true);
    } else {
      TI_ERROR("DB is unavailable!!!");
      return;
    }
  }

  if (!s.ok()) {
    TI_ERROR("backup failed: %s", s.ToString().c_str());
    return;
  }

#ifdef NEED_VERIFY_BACKUP
  std::vector<rocksdb::BackupInfo> backup_info;
  backup_engine->GetBackupInfo(&backup_info);
  size_t count = backup_info.size();
  for (int i = 0; i < count; i++) {
    s = backup_engine->VerifyBackup(backup_info[i].backup_id);
    if (!s.ok()) {
      std::cerr << s.ToString() << std::endl;
      delete backup_engine;
      return;
    }
  }
  // release memory
  backup_info.clear();
  std::vector<rocksdb::BackupInfo>().swap(backup_info);
#endif /* NEED_VERIFY_BACKUP */
  // TO DO by yzh
  // solve thread-safe problem
  // backup_engine->PurgeOldBackups(1);
}

void RocksDBRepositoryImpl::restore(void) {
  TI_INFO("start restore db!!!");

  std::unique_lock<std::shared_mutex> lock(mutex_);
  // close current db

  db_instance_.reset(nullptr);

  rocksdb::BackupEngineReadOnly* dummyBE;
  rocksdb::Status s = rocksdb::BackupEngineReadOnly::Open(
      rocksdb::Env::Default(),
      rocksdb::BackupableDBOptions(db_backup_file_, (rocksdb::Env*)nullptr,
                                   share_table_files_),
      &dummyBE);

  if (!s.ok()) {
    TI_ERROR("%s", s.ToString().c_str());
    return;
  }

  std::unique_ptr<rocksdb::BackupEngineReadOnly> backup_engine(dummyBE);

#ifdef NEED_VERIFY_BACKUP
  std::vector<rocksdb::BackupInfo> backup_info;
  backup_engine->GetBackupInfo(&backup_info);
  size_t count = backup_info.size();
  for (int i = 0; i < count; i++) {
    s = backup_engine->VerifyBackup(backup_info[i].backup_id);
    if (!s.ok()) {
      std::cerr << s.ToString() << std::endl;
      delete backup_engine;
      return;
    }
  }
  // release memory
  backup_info.clear();
  std::vector<rocksdb::BackupInfo>().swap(backup_info);
#endif /* NEED_VERIFY_BACKUP */

  s = backup_engine->RestoreDBFromLatestBackup(db_file_, db_file_);
  if (!s.ok()) {
    TI_ERROR("resotre error: %s", s.ToString().c_str());
  }

  // TO DO by yzh
  // reopen database and solve thread-safe problem
  // backup_engine->PurgeOldBackups(1);
  // load the last option file
  rocksdb::Options last_options;
  std::vector<rocksdb::ColumnFamilyDescriptor> loaded_cf_descs;
  rocksdb::LoadLatestOptions(db_file_, rocksdb::Env::Default(), &last_options,
                             &loaded_cf_descs);

  rocksdb::DB* dummy;
  s = rocksdb::DB::Open(last_options, db_file_, &dummy);
  if (!s.ok()) {
    TI_ERROR("reopen error: %s", s.getState());
    return;
  }

  db_instance_.reset(dummy);
  TI_INFO("reopen db success!!!");
}

}  // namespace ti