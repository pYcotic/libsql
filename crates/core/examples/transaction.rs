use libsql::Database;

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();

    let db_file = tempfile::NamedTempFile::new().unwrap();
    println!("Database {}", db_file.path().display());

    let auth_token = std::env::var("TURSO_AUTH_TOKEN").unwrap_or_else(|_| {
        println!("Using empty token since TURSO_AUTH_TOKEN was not set");
        "".to_string()
    });

    let db = Database::open_with_remote_sync(
        db_file.path().to_str().unwrap(),
        "http://localhost:8080",
        auth_token,
    )
    .await
    .unwrap();
    let conn = db.connect().unwrap();

    let tx = conn
        .transaction_with_behavior(libsql::TransactionBehavior::Immediate)
        .await
        .unwrap();

    tx.execute("SELECT 1", ()).await.unwrap();

    tx.commit().await.unwrap();
}
