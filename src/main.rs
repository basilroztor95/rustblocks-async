use rustblocks::{
    cli::{AtomBlocksCli, CliActions},
    config::Config,
    error::AtomBlocksError,
    types::Result,
    AtomBlocks,
};
use simple_logger::SimpleLogger;
use log::LevelFilter;
use std::path::PathBuf;

const CONFIG_FILE: &str = "config.toml";

fn main() {
    let cli: AtomBlocksCli = argh::from_env();

    // Initialize logger based on verbosity level
    let mut logger = SimpleLogger::new();

    if cli.trace() {
        logger = logger.with_level(LevelFilter::Trace);
    } else if cli.verbose() {
        logger = logger.with_level(LevelFilter::Info);
    } else {
        logger = logger.with_level(LevelFilter::Error);
    }

    logger.init().unwrap();
    log::debug!("Logger initialized");

    // Handle CLI actions
    if cli.version() {
        println!("{} {}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"));
        return;
    }

    let result = match cli.action() {
        Some(CliActions::Run(params)) => {
            let config_file = if let Some(path) = params.config() {
                Ok(path)
            } else {
                get_config_path()
            };
            log::info!("Starting AtomBlocks");
            if let Ok(config_file) = config_file {
                run(config_file)
            } else {
                Err(AtomBlocksError::Config("Failed to load config".into()))
            }
        }
        Some(CliActions::Hit(params)) => hit(params.id()),
        None => {
            println!("Missing command");
            Ok(())
        }
    };

    if let Err(err) = result {
        log::error!("{}", err);
        eprintln!("{}", err);
    }
}

fn run(config: PathBuf) -> rustblocks::types::Result<()> {
    log::debug!("Starting AtomBlocks");
    Config::load_from_file(config)
        .and_then(|config| AtomBlocks::new(config).and_then(|mut a| a.run()))
}

fn hit(id: u32) -> rustblocks::types::Result<()> {
    let hitman = rustblocks::HitMan::new()?;
    hitman.hit_block(id)
}

fn get_config_path() -> Result<PathBuf> {
    let mut path: PathBuf;
    if let Some(home_config_dir) = std::env::var_os("XDG_CONFIG_HOME") {
        path = PathBuf::from(home_config_dir);
        path.push("rustblocks");
    } else if let Some(home_dir) = std::env::var_os("HOME") {
        path = PathBuf::from(home_dir);
        path.push(".config/rustblocks");
    } else {
        path = PathBuf::from("/etc/rustblocks");
    }
    path.push(CONFIG_FILE);

    if !path.exists() {
        return Err(AtomBlocksError::Config(
            "Config file not specified or not found".to_owned(),
        ));
    }
    Ok(path)
}
