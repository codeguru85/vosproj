package dto.security.video
{
	import mx.collections.ArrayCollection;	
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoInfoBean")]
	public class VideoInfoBean
	{
		
		public var videoId:int;
		public var userInfo:UserInfo;
		public var videoName:String;
		public var createTime:Date;
		public var fileName:String;
		public var fileFormat:String;
		public var filePath:String;
		public var imageName:String;
		public var imagePath:String;
		public var fileSize:Number;
		public var timeCount:String;
		public var descn:String;
		public var setTime:String;
		
		public var startTime:String;
		public var endTime:String;
		public var startSize:String;
		public var endSize:String;
		public var startDate:Date;
		public var endDate:Date;
		
		public var keywords:ArrayCollection;
		public var videoTypes:ArrayCollection;
		public var videoAlbumShips:ArrayCollection;
		public var playListShips:ArrayCollection;
		
		public function VideoInfoBean()
		{
			//TODO: implement function
		}

	}
}